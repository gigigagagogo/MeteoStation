<?php

namespace App\Http\Middleware;

use Illuminate\Foundation\Http\Middleware\VerifyCsrfToken as Middleware;

class VerifyCsrfToken extends Middleware
{
    /**
     * Gli URI che devono essere esclusi dalla verifica CSRF.
     *
     * @var array
     */
    protected $except = [
        '/esp-data',
    ];
}
