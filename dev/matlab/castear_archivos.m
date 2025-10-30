%% Castear la señal de double a q15_t
temp_mV = ciclo_double;
ciclo_q15 = int16(round(temp_mV*(32768/3.3)));
%% Castear la señal de double a uint12_t para el DAC
offset = min(ciclo_double);
temp = ciclo_double + abs(offset);
ciclo_dac12 = uint16(round( temp * (4095 / 3.3)));

%% Generar archivos
save_q15 = save_ecg(ciclo_q15, hrmean, 'ecg_q15', 'q15_t');
save_dac = save_ecg(ciclo_dac12, hrmean, 'ecg_dac12b', 'uint16_t');
