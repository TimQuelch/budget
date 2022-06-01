from django.db import models


class Payee(models.Model):
    name = models.CharField(max_length=50, unique=True)

    def __str__(self):
        return str(self.name)


class Account(Payee):
    def __str__(self):
        return str(self.name)


class Transaction(models.Model):
    src = models.ForeignKey(Payee, on_delete=models.SET_NULL, related_name="outgoing_transactions", null=True)
    dst = models.ForeignKey(Payee, on_delete=models.SET_NULL, related_name="incoming_transactions", null=True)
    date = models.DateField()
    amount = models.DecimalField(decimal_places=2, max_digits=16)
    memo = models.CharField(max_length=100)

    def __str__(self):
        return "{}: ${} from '{}' to '{}' ({})".format(self.date, self.amount, self.src, self.dst, self.memo)
