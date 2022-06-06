from django.urls import path, include
from rest_framework import routers

from . import views

router = routers.DefaultRouter()
router.register(r'payees', views.PayeeViewSet)
router.register(r'accounts', views.AccountViewSet)

urlpatterns = [
    path('', include(router.urls))
]
