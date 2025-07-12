<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;
use Illuminate\Support\Facades\DB;
use Illuminate\Support\Facades\Log;

Route::get('/user', function (Request $request) {
    return $request->user();
})->middleware('auth:sanctum');

Route::post('/esp-data', function (Request $request) {

    Log::info('ESP32 data:', $request->all());

    $lat = $request->input('lat');
    $lon = $request->input('lon');
    $temp = $request->input('temp');
    $pres = $request->input('pres');
    $humi = $request->input('humi');

    if (!is_null($lat) && !is_null($lon) && !is_null($temp) && !is_null($pres) && !is_null($humi)) {
        DB::table('parameters')->insert([
            'temperature' => $temp,
            'pressure' => $pres,
            'humidity' => $humi,
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


