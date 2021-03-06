from rest_framework import serializers

from backend.models import Payee, BudgetMonth, BudgetEntry, Transaction


class PayeeSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = Payee
        fields = ["url", "name"]


class AccountSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = Payee
        fields = ["url", "name", "is_tracking_account", "is_budget_account", "balance"]
        read_only_fields = ["balance"]


class BudgetEntrySerializer(serializers.ModelSerializer):
    category = serializers.SlugRelatedField(read_only=True, slug_field="name")

    class Meta:
        model = BudgetEntry
        fields = ["category", "allocated", "balance", "activity"]


class BudgetMonthSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = BudgetMonth
        fields = ["url", "month"]
        extra_kwargs = {"url": {"lookup_field": "month"}}


class BudgetMonthDetailSerializer(BudgetMonthSerializer):
    entries = BudgetEntrySerializer(many=True)

    class Meta:
        model = BudgetMonth
        fields = ["url", "month", "entries"]
        extra_kwargs = {"url": {"lookup_field": "month"}}


class TransactionSerializer(serializers.HyperlinkedModelSerializer):
    src = serializers.SlugRelatedField(read_only=True, slug_field="name")
    dst = serializers.SlugRelatedField(read_only=True, slug_field="name")
    category = serializers.SlugRelatedField(read_only=True, slug_field="name")

    class Meta:
        model = Transaction
        fields = ["url", "date", "src", "dst", "category", "amount", "memo"]
