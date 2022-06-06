from rest_framework import serializers
from backend.models import Payee


class PayeeSerializer(serializers.HyperlinkedModelSerializer):
    class Meta:
        model = Payee
