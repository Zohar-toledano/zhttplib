param(
	$name
)
g++ ".\$name.cpp" -o $name;
if($LASTEXITCODE -ne 0)
{
	Write-Host error!;
}
else
{
	Write-Host "running ./$name.exe"...;
	
	&"./$name.exe"
	Remove-Item "./$name.exe"
}
