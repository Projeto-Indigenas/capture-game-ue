. ".\DevOps\Library.ps1"

$serverMap = "/Game/StackOBot/World/L_TestMap.umap"
$clientMap = "/Game/StackOBot_Old/UI/MainMenu/MainMenu.umap"

.\DevOps\BuildProject.ps1

Write-Output "Starting server instance"

RunInstance $serverMap $true

if ($args.Count -gt 0)
{
    for($i = 0; $i -lt $args[0]; $i++)
    {
        Write-Output "Starting client instance $i"
        
        RunInstance $clientMap $false
    }
}
