%% Insertar parámetros de ECG a simular
sfecg = 1000;                       % Frecuencia de muestreo
N = 32;                             % Número de latidos a simular
Anoise = 0;                         % Amplitud del ruido [mV]
hrmean = 60;                       % Frecuencia cardíaca media [bpm]
hrstd = 0;                          % Variabilidad de la frecuencia cardíaca
lfhfratio = 0;                      % Relación LF/HF
sfint = 1000;                       % Frecuencia de muestreo interna
ti = [-60 -8 0 8 90];               % Ángulos de los atractores
ai = [1.2 -5 30 -7.5 0.75];         % Amplitudes de los atractores
bi = [0.15 0.08 0.08 0.08 0.25];    % Ancho de los atractores
fresp = 0;                          % Frecuencia del ruido despiratorio (Hz)
flo = 0;
fhi = 0;
flostd = 1e-9;
fhistd = 1e-9;

%% Generar ECG
[s, ipeaks] = ecgsyn(sfecg,N,Anoise,hrmean,hrstd,lfhfratio,sfint,ti,ai,bi,fresp, flo, fhi, flostd, fhistd);

%% Aislar un ciclo
hrmean_Hz = hrmean/60;
samples_per_cycle = round(sfecg/hrmean_Hz);

R = find(ipeaks == 3);
start_qrs = R(10);
end_qrs = R(11) - 1;

ciclo_double = s(start_qrs + round(samples_per_cycle/2) : end_qrs + round(samples_per_cycle/2));

%% Generar archivos
save_double = save_ecg(ciclo_double, hrmean, 'ecg_double', 'double');


