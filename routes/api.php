<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;
use Illuminate\Support\Facades\DB;
use Illuminate\Support\Facades\Log;

Route::get('/user', function (Request $request) {
    return $request->user();
})->middleware('auth:sanctum');

Route::post('/esp-data', function (Request $request) {
    $lat = $request->input('lat');
    $lon = $request->input('lon');

    if ($lat && $lon) {
        DB::table('parameters')->insert([
            'air_quality' => 0,
            'temperature' => 0,
            'pressure' => 0,
            'umidity' => 0,
            'latitude' => $lat,
            'longitude' => $lon,
            'created_at' => now(),
            'updated_at' => now()
        ]);
        return response("OK", 200);
    } else {
        return response("Dati mancanti", 400);
    }
});


