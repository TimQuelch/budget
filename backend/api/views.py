from rest_framework import viewsets

from backend.models import Payee, BudgetMonth, Transaction
from . import serializers


class PayeeViewSet(viewsets.ModelViewSet):
    queryset = Payee.payees.all()
    serializer_class = serializers.PayeeSerializer


class AccountViewSet(viewsets.ModelViewSet):
    queryset = Payee.accounts.all()
    serializer_class = serializers.AccountSerializer


class BudgetMonthViewSet(viewsets.ModelViewSet):
    queryset = BudgetMonth.objects.all()
    lookup_field = "month"

    def get_serializer_class(self):
        if self.action == "list":
            return serializers.BudgetMonthSerializer

        return serializers.BudgetMonthDetailSerializer


class TransactionViewSet(viewsets.ModelViewSet):
    queryset = Transaction.objects.all()
    serializer_class = serializers.TransactionSerializer
