from rest_framework import serializers
from ledger.models import Payee


class PayeeSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = Payee
