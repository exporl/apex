<?php

$response = array('output' => array());
function addError(&$response, $error) {
    if (!array_key_exists('errors', $response))
        $response += array('errors' => array());
    array_push($response['errors'], $error);
}
if (empty($_REQUEST['pubkey']))
    addError($response, 'Public key is empty');
if (array_key_exists('errors', $response)) {
    echo(json_encode($response));
    exit(1);
}

$addDeviceCommand = sprintf('admin-scripts/add-device.sh --yes'
                            . ' --ssh-config "/var/www/apex-study/admin-scripts/ssh-config"'
                            . ' --key %s', escapeshellarg($_REQUEST['pubkey']));
$addDeviceOutput = array();
$addDeviceExit = 0;
exec($addDeviceCommand, $addDeviceOutput, $addDeviceExit);

array_push($response['output'], $addDeviceCommand);
$response['output'] = array_merge($response['output'], $addDeviceOutput);
if ($addDeviceExit != 0)
    addError($response, 'Unable to create device, see output for details');

echo(json_encode($response));

?>
