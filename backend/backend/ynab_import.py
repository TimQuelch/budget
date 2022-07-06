from django.db.models import Count

import pandas as pd
import re

from . import models
from .util import update_all_balances


def create_payees(d):
    payee_names = d.Payee.unique()
    transfer_re = re.compile(r"Transfer : .*")
    non_transfer_payees = filter(lambda n: not transfer_re.match(n), payee_names)
    models.Payee.objects.bulk_create(
        [models.Payee(name=n) for n in non_transfer_payees]
    )


def create_accounts(d):
    account_names = d.Account.unique()

    # Right now in YNAB accounts and Payees can have the same names. This will throw an error below
    # (for now, as payee/account name is unique)

    # bulk_create does not work for child models
    for n in account_names:
        try:
            models.Payee(name=n, acctype=models.AccType.BUDGET_ACCOUNT).save()
        except:
            print(n)
            raise


def create_transactions(d):
    # Doesn't handle double counting of inter-account transfers

    payees = models.Payee.objects.in_bulk(field_name="name")
    categories = models.Category.objects.in_bulk(field_name="name")
    d["PayeeStripped"] = d.Payee.str.replace("Transfer : ", "", regex=False)

    def create_transaction(it):
        t = it[1]
        if t.Inflow > 0:
            assert t.Outflow == 0
            srcname = t.PayeeStripped
            dstname = t.Account
            amount = t.Inflow
        else:
            assert t.Inflow == 0
            srcname = t.Account
            dstname = t.PayeeStripped
            amount = t.Outflow

        src = payees[srcname]
        dst = payees[dstname]
        categories["Ready to Assign"] = categories["Inflow"]
        category = categories[t.Category] if t.Category else None

        return models.Transaction(
            src=src,
            dst=dst,
            date=t.Date,
            amount=amount,
            memo=t.Memo,
            category=category,
        )

    models.Transaction.objects.bulk_create(map(create_transaction, d.iterrows()))


def remove_dupes():
    # Transfers are between accounts. They are double counted in import, so we need to delete one
    dupes = (
        models.Transaction.transfers.values("src", "dst", "amount", "date")
        .annotate(Count("id"))
        .order_by()
        .filter(id__count__gt=1)
    )

    for vs in dupes:
        # If there are identical transfers on the same day we'll need to change this
        assert vs["id__count"] == 2

        # Find the pair of transactions
        pair = models.Transaction.objects.filter(
            amount=vs["amount"], date=vs["date"], src_id=vs["src"], dst_id=vs["dst"]
        )

        # Hopefully we don't have multiple transactions between the same accounts on the same day
        # with the same amount. This is possible, but I don't have any for now so I'm ignoring the
        # possibility
        assert len(pair) == 2

        # For transfers to tracking accounts, one of the transactions has a category and the other
        # one does not. We want to retain the transaction that has the category and delete the other
        if pair[0].category is not None:
            pair[1].delete()
        else:
            pair[0].delete()

    # Verify there are none remaining
    ndupes = len(
        models.Transaction.transfers.values("src", "dst", "amount", "date")
        .annotate(Count("id"))
        .order_by()
        .filter(id__count__gt=1)
    )
    assert ndupes == 0


def create_categories(b):
    names = b.Category.unique()
    models.Category.objects.bulk_create([models.Category(name=n) for n in names])
    models.Category.objects.create(name="Inflow").save()


def create_months(b):
    months = b.Month.unique()
    # Some bananas conversions required here between numpy, pandas, and python datetime
    models.BudgetMonth.objects.bulk_create(
        [models.BudgetMonth(month=pd.to_datetime(m).date()) for m in months]
    )


def create_budget_entries(b):
    categories = models.Category.objects.in_bulk(field_name="name")
    months = models.BudgetMonth.objects.in_bulk(field_name="month")

    def create_entry(ib):
        b = ib[1]
        bobj = models.BudgetEntry(
            month=months[pd.to_datetime(b.Month).date()],
            category=categories[b.Category],
            allocated=b.Budgeted,
        )
        bobj.full_clean()
        return bobj

    bs = list(map(create_entry, b.iterrows()))
    models.BudgetEntry.objects.bulk_create(map(create_entry, b.iterrows()))


def set_tracking_accounts(acc_names):
    tracking_accounts = models.Payee.accounts.filter(name__in=acc_names)
    assert len(acc_names) == len(tracking_accounts)
    tracking_accounts.update(acctype=models.AccType.TRACKING_ACCOUNT)


def import_csv(
    register_filename, budget_filename, clear_table=False, tracking_accounts=[]
):
    r = pd.read_csv(
        register_filename,
        parse_dates=[2],
        infer_datetime_format=True,
        converters={"Memo": str, "Category": str},
    )
    r.Outflow = r.Outflow.str.replace("$", "", regex=False).apply(lambda x: float(x))
    r.Inflow = r.Inflow.str.replace("$", "", regex=False).apply(lambda x: float(x))

    b = pd.read_csv(
        budget_filename,
        parse_dates=[0],
        infer_datetime_format=True,
        converters={"Category": str},
    )
    b.Budgeted = b.Budgeted.str.replace("$", "", regex=False).apply(lambda x: float(x))

    # Wrap all in transaction?

    if clear_table:
        models.Payee.objects.all().delete()
        models.Transaction.objects.all().delete()
        models.Category.objects.all().delete()
        models.BudgetMonth.objects.all().delete()
        models.BudgetEntry.objects.all().delete()

    create_payees(r)
    create_accounts(r)

    create_categories(b)
    create_months(b)
    create_budget_entries(b)

    create_transactions(r)
    remove_dupes()

    set_tracking_accounts(tracking_accounts)

    update_all_balances()

    return r, b
