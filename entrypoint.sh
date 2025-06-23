#!/bin/sh

# Aspetta che il DB sia disponibile (opzionale)
if [ -f /usr/local/bin/wait-for-mysql.sh ]; then
  /usr/local/bin/wait-for-mysql.sh
fi

# Copia .env.example in .env se non esiste
if [ ! -f .env ]; then
  cp .env.example .env
fi

# Genera la APP_KEY se non ancora settata
if ! grep -q "APP_KEY=base64:" .env; then
  php artisan key:generate
fi

# Avvia il server Laravel
php artisan serve --host=0.0.0.0 --port=8000
