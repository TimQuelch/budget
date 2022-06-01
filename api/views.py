from django.shortcuts import render
from rest_framework import viewsets
from ledger.models import Payee

from . import serializers


class PayeeViewSet(viewsets.ModelViewSet):
    queryset = Payee.objects.all()
    serializer_class = serializers.PayeeSerializer
