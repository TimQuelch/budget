from datetime import date, timedelta
import calendar

from django.db import models


def previous_month(d):
    first_of_month = d.replace(day=1)
    last_of_previous_month = first_of_month - timedelta(days=1)
    first_of_previous_month = last_of_previous_month.replace(day=1)
    return first_of_previous_month


def next_month(d):
    last_of_month = d.replace(day=calendar.monthrange(d.year, d.month)[1])
    first_of_next_month = last_of_month + timedelta(days=1)
    return first_of_next_month


class Payee(models.Model):
    name = models.CharField(max_length=50, unique=True)
    is_account = models.BooleanField(default=False, editable=False)

    def __str__(self):
        return str(self.name)

    def balance(self, before=date.today()):
        def transaction_sum(qs):
            return sum(v["amount"] for v in qs.values("amount"))

        incoming = self.incoming_transactions.filter(date__lte=before)
        outgoing = self.outgoing_transactions.filter(date__lte=before)

        return round(transaction_sum(incoming) - transaction_sum(outgoing), 2)


class Account(Payee):
    def clean(self):
        super().clean()
        self.is_account = True

    def __str__(self):
        return str(self.name)


class Category(models.Model):
    name = models.CharField(max_length=50, unique=True)

    def __str__(self):
        return str(self.name)


class TransferManager(models.Manager):
    def get_queryset(self):
        return super().get_queryset().filter(src__is_account=True, dst__is_account=True)


class Transaction(models.Model):
    src = models.ForeignKey(
        Payee,
        on_delete=models.SET_NULL,
        related_name="outgoing_transactions",
        null=True,
    )
    dst = models.ForeignKey(
        Payee,
        on_delete=models.SET_NULL,
        related_name="incoming_transactions",
        null=True,
    )
    category = models.ForeignKey(
        Category,
        on_delete=models.SET_NULL,
        null=True,
    )
    date = models.DateField()
    amount = models.FloatField()
    memo = models.CharField(max_length=100)

    objects = models.Manager()
    transfers = TransferManager()

    @property
    def is_transfer(self):
        return self.src.is_account and self.dst.is_account

    @property
    def is_outgoing(self):
        return self.src.is_account and not self.dst.is_account

    @property
    def is_incoming(self):
        return not self.src.is_account and self.dst.is_account

    def clean(self):
        # Remove category if it is a transfer
        if self.is_transfer():
            self.category = None

    def __str__(self):
        return f"{self.date}: ${self.amount} from '{self.src}' to '{self.dst}' ({self.memo})"

    class Meta:
        indexes = [
            models.Index(fields=("category", "date"), name="transaction_category_date")
        ]


class BudgetMonth(models.Model):
    month = models.DateField(unique=True)

    def __str__(self):
        return self.month.strftime("%b %Y")

    def clean(self):
        # We don't care about the day of the entry
        if self.month.day != 1:
            self.month = self.month.replace(day=1)


class BudgetEntry(models.Model):
    month = models.ForeignKey(
        BudgetMonth,
        on_delete=models.CASCADE,
        related_name="entries",
    )
    category = models.ForeignKey(
        Category,
        on_delete=models.CASCADE,
        related_name="entries",
    )
    allocated = models.FloatField()
    balance = models.FloatField(editable=False, default=0)

    def __clamp_date_to_month(self, before):
        month = self.month.month
        end_of_month = date(
            month.year, month.month, calendar.monthrange(month.year, month.month)[1]
        )
        start_of_month = date(month.year, month.month, 1)
        return max(min(before, end_of_month), start_of_month)

    def update_balance(self, before=date.today(), update_future=True):
        before = self.__clamp_date_to_month(before)

        try:
            previous_entry = BudgetEntry.objects.get(
                category=self.category, month__month=previous_month(before)
            )
            opening_balance = previous_entry.balance
        except BudgetEntry.DoesNotExist:
            start_of_month = self.month.month
            transactions = self.category.transaction_set.filter(
                date__lt=start_of_month,
            ).select_related("src", "dst")
            opening_balance = sum(
                ((2 * t.is_outgoing - 1) * t.amount) for t in transactions
            )

        new_balance = round(opening_balance + self.allocated - self.activity(), 2)
        if new_balance != self.balance:
            self.balance = new_balance
            self.save()

        if update_future:
            try:
                next_entry = BudgetEntry.objects.get(
                    category=self.category, month__month=next_month(before)
                )
                next_entry.update_balance()
            except BudgetEntry.DoesNotExist:
                pass

    def activity(self, before=date.today()):
        before = self.__clamp_date_to_month(before)
        start_of_month = self.month.month
        transactions = self.category.transaction_set.filter(
            date__gte=start_of_month,
            date__lte=before,
        ).select_related("src", "dst")
        transaction_sum = sum(
            ((2 * t.is_outgoing - 1) * t.amount) for t in transactions
        )
        return round(transaction_sum, 2)

    def __str__(self):
        return f"{self.month} {self.category} Allocated ${self.allocated}"

    class Meta:
        constraints = [
            models.UniqueConstraint(
                fields=["month", "category"], name="unique_budget_entry"
            )
        ]
