<?php

use Illuminate\Support\Facades\Route;
use App\Http\Controllers\MeteoController;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\DB;


Route::get('/', [MeteoController::class, 'param']);

?>