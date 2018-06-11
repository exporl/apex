<?php

$response = array();
$response += array('personal' => array());
$response += array('tablets' => array());
$response += array('remaining' => array());

$listDevicesCommand = 'admin-scripts/list-devices.sh'
                    . ' --ssh-config "/var/www/apex-study/admin-scripts/ssh-config"';

if (array_key_exists('study', $_REQUEST))
    $listDevicesCommand .= ' --study ' . escapeshellarg($_REQUEST['study']);

$devices = array_filter(explode("\n", shell_exec($listDevicesCommand)));
foreach ($devices as $device) {
    if ($_SERVER['Shib-Person-uid']
        && substr($device, 0, strlen($_SERVER['Shib-Person-uid']))
        == $_SERVER['Shib-Person-uid'])
        array_push($response['personal'], $device);
    else
        array_push($response['remaining'], $device);
}

echo(json_encode($response));

?>
