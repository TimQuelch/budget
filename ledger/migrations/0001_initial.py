# Generated by Django 4.0.4 on 2022-06-01 07:38

from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Payee',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('name', models.CharField(max_length=50, unique=True)),
            ],
        ),
        migrations.CreateModel(
            name='Account',
            fields=[
                ('payee_ptr', models.OneToOneField(auto_created=True, on_delete=django.db.models.deletion.CASCADE, parent_link=True, primary_key=True, serialize=False, to='ledger.payee')),
            ],
            bases=('ledger.payee',),
        ),
        migrations.CreateModel(
            name='Transaction',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('date', models.DateField()),
                ('amount', models.DecimalField(decimal_places=2, max_digits=16)),
                ('memo', models.CharField(max_length=100)),
                ('dst', models.ForeignKey(null=True, on_delete=django.db.models.deletion.SET_NULL, related_name='incoming_transactions', to='ledger.payee')),
                ('src', models.ForeignKey(null=True, on_delete=django.db.models.deletion.SET_NULL, related_name='outgoing_transactions', to='ledger.payee')),
            ],
        ),
    ]
