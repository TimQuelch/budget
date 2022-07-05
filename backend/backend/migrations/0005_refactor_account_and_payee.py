# Generated by Django 4.0.5 on 2022-07-05 07:00

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('backend', '0004_use_float_instead_of_decimal'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='payee',
            name='is_account',
        ),
        migrations.AddField(
            model_name='payee',
            name='acctype',
            field=models.IntegerField(choices=[(1, 'Payee'), (2, 'Tracking Account'), (3, 'Budget Account')], default=1),
        ),
        migrations.DeleteModel(
            name='Account',
        ),
    ]
