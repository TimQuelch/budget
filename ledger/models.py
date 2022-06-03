from django.db import models

from datetime import date
from decimal import *


class Payee(models.Model):
    name = models.CharField(max_length=50, unique=True)

    def __str__(self):
        return str(self.name)


class Account(Payee):
    def __str__(self):
        return str(self.name)

    def balance(self, exclude_future=True):
        def transaction_sum(qs):
            return sum(v["amount"] for v in qs.values("amount"))

        incoming = self.incoming_transactions
        outgoing = self.outgoing_transactions

        if exclude_future:
            incoming = incoming.filter(date__lte=date.today())
            outgoing = outgoing.filter(date__lte=date.today())

        return transaction_sum(incoming) - transaction_sum(outgoing)


class TransferManager(models.Manager):
    def get_queryset(self):
        return (
            super()
            .get_queryset()
            .filter(src__account__isnull=False, dst__account__isnull=False)
        )


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
    date = models.DateField()
    amount = models.DecimalField(decimal_places=2, max_digits=16)
    memo = models.CharField(max_length=100)

    objects = models.Manager()
    transfers = TransferManager()

    def __str__(self):
        return "{}: ${} from '{}' to '{}' ({})".format(
            self.date, self.amount, self.src, self.dst, self.memo
        )
