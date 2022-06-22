from django.db.models import Min

from .models import BudgetEntry, Category


def update_category_balances(category):
    category = (
        Category.objects.filter(pk=category.pk)
        .annotate(min_month=Min("entries__month__month"))
        .get()
    )

    try:
        BudgetEntry.objects.get(
            category=category, month__month=category.min_month
        ).update_balance(update_future=True)
    except BudgetEntry.DoesNotExist:
        # If entries for the category exist, but one for the min_month doesn't something broke
        if category.entries.exists():
            raise


def update_all_balances():
    for category in Category.objects.all():
        update_category_balances(category)
