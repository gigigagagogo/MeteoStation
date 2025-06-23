<!DOCTYPE html>
<html lang="it">
<head>
    <meta charset="UTF-8">
    <title>Stazioni Meteo</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet">
    <link href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.3/font/bootstrap-icons.css" rel="stylesheet">
    <link href="{{ asset('css/style.css')  }}" rel="stylesheet">
</head>
<body>

<div class="container py-5">
    <h2 class="text-center mb-5 display-6 fw-bold text-primary">📡 Dati Stazioni Meteo</h2>

    @foreach ($stazioni as $s)
        <div class="weather-card p-4 mb-4 rounded-4 shadow-sm">
            <h5 class="mb-3 text-dark">
                <i class="bi bi-geo-alt icon"></i>
                <strong>{{ $s->latitude }}, {{ $s->longitude }}</strong>
            </h5>
            <ul class="list-inline mb-0 d-flex flex-wrap gap-4 justify-content-start">
                <li class="list-inline-item">
                    <i class="bi bi-wind icon"></i> Qualità Aria: <strong>{{ $s->air_quality }}</strong>
                </li>
                <li class="list-inline-item">
                    <i class="bi bi-thermometer-half icon"></i> Temperatura: <strong>{{ $s->temperature }}°C</strong>
                </li>
                <li class="list-inline-item">
                    <i class="bi bi-speedometer icon"></i> Pressione: <strong>{{ $s->pressure }} hPa</strong>
                </li>
                <li class="list-inline-item">
                    <i class="bi bi-droplet-half icon"></i> Umidità: <strong>{{ $s->umidity }}%</strong>
                </li>
            </ul>
        </div>
    @endforeach

</div>

<script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/js/bootstrap.bundle.min.js"></script>
</body>
</html>
