<?php

namespace App\Http\Controllers;
use Illuminate\Support\Facades\DB;
use Illuminate\Http\Request;

class MeteoController extends Controller{
    public function param(){
        $stazioni = DB::table('parameters')->select('*')->get();
        return view('home',['stazioni'=>$stazioni]);
    }
}

