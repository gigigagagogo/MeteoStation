FROM php:8.2-fpm-alpine

# Installa le dipendenze di sistema necessarie per Laravel e Composer
RUN apk update && apk add --no-cache \
  libzip-dev \
  curl \
  bash \
  mysql-client && \
  rm -rf /var/cache/apk/*

# Installa Composer
RUN curl -sS https://getcomposer.org/installer | php -- --install-dir=/usr/local/bin --filename=composer

# Installa le estensioni PHP necessarie per Laravel
RUN docker-php-ext-configure zip && \
  docker-php-ext-install zip pdo pdo_mysql

# Imposta la directory di lavoro
WORKDIR /var/www/html

# Copia tutto tranne i file nel .dockerignore
COPY . .

# Installa le dipendenze di Laravel (incluso Composer)
RUN composer install --no-dev --optimize-autoloader --prefer-dist

# Esponi la porta su cui il server Laravel girera
EXPOSE 8000

# Comando per eseguire Laravel, con attesa per MySQL
COPY entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

# Usa lo script come comando iniziale
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
