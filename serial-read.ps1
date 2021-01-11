$port = new-Object System.IO.Ports.SerialPort COM3, 115200, None, 8, one
$port.Open()
try {
    do {
        $line = $port.ReadLine()
        Write-Output $line
    }
    while ($port.IsOpen)
}
catch [System.Management.Automation.MethodInvocationException] {
    $isError = $true
    Write-Output "Error: port disconnected"
}
finally {
    if ($port.IsOpen) {
        $port.Close()
    }
    if (!$isError) {
        Write-Output "Monitor done" | Out-Default
    }
}