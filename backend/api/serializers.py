from rest_framework import serializers

from backend.models import Payee, Account, BudgetMonth, BudgetEntry


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


class BudgetEntrySerializer(serializers.ModelSerializer):
    category = serializers.SlugRelatedField(read_only=True, slug_field="name")
    class Meta:
        model = BudgetEntry
        fields = ["category", "allocated"]


class BudgetMonthSerializer(serializers.HyperlinkedModelSerializer):
    entries = BudgetEntrySerializer(many=True)
    class Meta:
        model = BudgetMonth
        fields = ["url", "month", "entries"]
        extra_kwargs = {
            "url": {"lookup_field": "month"}
        }
