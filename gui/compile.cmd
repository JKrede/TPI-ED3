@echo off
setlocal

rem 1) Generar ui.py desde ui.ui usando el Python del venv si existe
set "PY=.venv\Scripts\python.exe"
if exist "%PY%" (
  "%PY%" -m PyQt5.uic.pyuic -x ui.ui -o ui.py
) else (
  python -m PyQt5.uic.pyuic -x ui.ui -o ui.py
)

rem 2) Insertar las conexiones ANTES de self.retranslateUi(MainWindow), línea por línea
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$p='ui.py';" ^
  "$lines = Get-Content $p;" ^
  "$out = @();" ^
  "foreach($l in $lines){" ^
  "  if($l -match '^\s*self\.retranslateUi\(MainWindow\)'){ " ^
  "    $indent = ($l -replace '(\S.*)$','');" ^
  "    $out += ($indent + 'self.connectUARTpushButton.clicked.connect(self.connectUARTpushButton_clicked)');" ^
  "    $out += ($indent + 'self.disconnectPushButton.clicked.connect(self.disconnectPushButton_clicked)');" ^
  "  }" ^
  "  $out += $l" ^
  "};" ^
  "$out | Set-Content -Encoding UTF8 $p;"

endlocal
pause
