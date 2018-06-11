<?php

if (!array_key_exists('Shib-Person-mail', $_SERVER)) {
    echo('doesnt exist');
    exit (1);
}

$accountExistsCommand =
    sprintf('admin-scripts/account-exists.sh'.' --full-name %s --email %s'
                .' --ssh-config "/var/www/apex-study/admin-scripts/ssh-config"',
            escapeshellarg($_SERVER['Shib-Person-commonName']),
            escapeshellarg($_SERVER['Shib-Person-mail']));
echo(json_encode(array_filter(explode("\n",
                                      shell_exec($accountExistsCommand)))));
?>
