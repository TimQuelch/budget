from django.db.models import Count

import pandas as pd
import re
from decimal import *

from . import models


def create_payees(d):
    payee_names = d.Payee.unique()
    transfer_re = re.compile(r"Transfer : .*")
    non_transfer_payees = filter(lambda n: not transfer_re.match(n), payee_names)
    models.Payee.objects.bulk_create(
        [models.Payee(name=n) for n in non_transfer_payees]
    )


def create_accounts(d):
    account_names = d.Account.unique()

    # bulk_create does not work for child models
    for n in account_names:
        try:
            models.Account(name=n).save()
        except:
            print(n)
            raise


def create_transactions(d):
    # Doesn't handle double counting of inter-account transfers

    payees = models.Payee.objects.in_bulk(field_name="name")
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

        return models.Transaction(
            src=src, dst=dst, date=t.Date, amount=amount, memo=t.Memo
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

        assert len(pair) == 2

        # Delete the 2nd transaction
        pair[1].delete()

    # Verify there are none remaining
    ndupes = len(
        models.Transaction.transfers.values("src", "dst", "amount", "date")
        .annotate(Count("id"))
        .order_by()
        .filter(id__count__gt=1)
    )
    assert ndupes == 0

def import_csv(filename, clear_table=False):
    d = pd.read_csv(
        filename, parse_dates=[2], infer_datetime_format=True, converters={"Memo": str}
    )
    d.Outflow = d.Outflow.str.replace("$", "", regex=False).apply(lambda x: Decimal(x))
    d.Inflow = d.Inflow.str.replace("$", "", regex=False).apply(lambda x: Decimal(x))

    # Wrap all in transaction?

    if clear_table:
        models.Account.objects.all().delete()
        models.Payee.objects.all().delete()
        models.Transaction.objects.all().delete()

    create_payees(d)
    create_accounts(d)
    create_transactions(d)

    remove_dupes()
