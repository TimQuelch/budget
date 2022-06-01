from django.urls import path, include
from rest_framework import routers

from . import views

router = routers.DefaultRouter()
router.register(r'ledger', views.PayeeViewSet)

urlpatterns = [
    path('', include(router.urls))
]
