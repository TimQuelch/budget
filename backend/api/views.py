from rest_framework import viewsets

from backend.models import Payee, Account, BudgetMonth
from . import serializers


class PayeeViewSet(viewsets.ModelViewSet):
    queryset = Payee.objects.all()
    serializer_class = serializers.PayeeSerializer


class AccountViewSet(viewsets.ModelViewSet):
    queryset = Account.objects.all()
    serializer_class = serializers.AccountSerializer


class BudgetMonthViewSet(viewsets.ModelViewSet):
    queryset = BudgetMonth.objects.all()
    lookup_field = "month"

    def get_serializer_class(self):
        if self.action == 'list':
            return serializers.BudgetMonthSerializer

        return serializers.BudgetMonthDetailSerializer
