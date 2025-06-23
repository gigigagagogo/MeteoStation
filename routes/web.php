<?php

use Illuminate\Support\Facades\Route;
use App\Http\Controllers\MeteoController;

Route::get('/', [MeteoController::class, 'param'])->name('param');

?>