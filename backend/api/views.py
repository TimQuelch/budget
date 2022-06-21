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
    serializer_class = serializers.BudgetMonthSerializer
    lookup_field = "month"
