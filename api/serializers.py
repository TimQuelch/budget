from rest_framework import serializers

from backend.models import Payee, Account


class AccountSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = Account
        fields = ["url", "name", "balance"]
        read_only_fields = ["balance"]


class PayeeSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = Payee
        fields = ["url", "name", "is_account", "balance"]
        read_only_fields = ["balance"]
