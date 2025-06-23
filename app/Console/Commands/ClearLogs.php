<?php

namespace App\Console\Commands;

use Illuminate\Console\Command;
use Illuminate\Support\Facades\File;

class ClearLogs extends Command
{
    protected $signature = 'clear:logs';
    protected $description = 'Svuota tutti i file di log nella cartella storage/logs';

    public function handle()
    {
        $logFiles = File::files(storage_path('logs'));

        foreach ($logFiles as $file) {
            File::put($file->getPathname(), '');
        }

        $this->info('Tutti i file di log sono stati svuotati.');
    }
}
