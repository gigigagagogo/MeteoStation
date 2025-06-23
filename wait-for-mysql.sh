#!/bin/bash
# Impostiamo PMA_HOST a "db" se non è già definito
: ${PMA_HOST:="db"}

# Aspetta che MySQL sia pronto
until nc -z -v -w30 "$PMA_HOST" 3306; do
  echo "Waiting for MySQL at $PMA_HOST:3306..."
  sleep 1
done
echo "MySQL is up - continuing"

