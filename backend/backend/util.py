from django.db.models import Min

from . import models


def update_category_balances(category):
    category = (
        models.Category.objects.filter(pk=category.pk)
        .annotate(min_month=Min("entries__month__month"))
        .get()
    )

    try:
        models.BudgetEntry.objects.get(
            category=category, month__month=category.min_month
        ).update_balance(update_future=True)
    except models.BudgetEntry.DoesNotExist:
        # If entries for the category exist, but one for the min_month doesn't something broke
        if category.entries.exists():
            raise


def update_all_balances():
    for category in models.Category.objects.all():
        update_category_balances(category)
