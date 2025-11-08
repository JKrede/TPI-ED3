function outPath = save_ecg(ciclo, hrmean, outDir, cType)
%SAVE_ECG Guarda un array en formato C dentro de una carpeta.
%   outPath = save_ecg_double(ciclo, hrmean)
%   outPath = save_ecg_double(ciclo, hrmean, outDir, varPrefix, valuesPerLine, precision)
%
%   - ciclo  : vector de datos
%   - hrmean        : frecuencia cardiaca media (ppm) usada para nombrar archivo/variable
%   - outDir        : carpeta de salida (default: 'ecg_double')
%   - varPrefix     : prefijo del nombre de variable (default: 'ecg')
%   - cType         : tipo en C (ej: 'double', 'q15_t', 'uint16_t')

    % -------- Validaciones básicas --------
    if ~isvector(ciclo) || ~isnumeric(ciclo)
        error('ciclo debe ser un vector numérico.');
    end
    if ~isscalar(hrmean) || ~isnumeric(hrmean)
        error('hrmean debe ser un escalar numérico.');
    end

    valuesPerLine = 10;
    precision = 6;
    varPrefix = 'ecg';

    % -------- Preparar nombres --------
    ppm     = round(hrmean);
    varName = sprintf('%s_%dppm', varPrefix, ppm);
    if ~exist(outDir, 'dir'), mkdir(outDir); end
    outPath = fullfile(outDir, sprintf('ecg_%s_%dppm.txt', cType, ppm));

    % -------- Abrir archivo --------
    fid = fopen(outPath, 'w');
    if fid == -1
        error('No se pudo abrir %s para escritura.', outPath);
    end

    % -------- Encabezado C --------
    N = numel(ciclo);
    fprintf(fid, 'const %s %s[%d] = {\n', cType, varName, N);

    % -------- Selección de formato --------
    if any(strcmpi(cType, {'q15_t','uint16_t'}))
        valFmt = '%d';     % enteros, sin decimales
        useCommas = false; % sin comas
    else
        valFmt = sprintf('%%.%df', precision); % flotantes con decimales
        useCommas = true;  % con comas
    end

     % -------- Escribir datos --------
    for i = 1:N
        isFirstInLine = mod(i-1, valuesPerLine) == 0;
        isLast        = (i == N);

        if isFirstInLine
            fprintf(fid, '    '); % indentación
        end

        if ~isLast
            fprintf(fid, [valFmt, ', '], ciclo(i));
        else
            fprintf(fid, valFmt, ciclo(i));
            if ~isLast, fprintf(fid, ' '); end
        end

        if mod(i, valuesPerLine) == 0 || isLast
            fprintf(fid, '\n');
        end
    end


    % -------- Cierre --------
    fprintf(fid, '};\n');
    fclose(fid);
end
