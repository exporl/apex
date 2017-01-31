function Controller() {
    installer.autoRejectMessageBoxes();
    installer.installationFinished.connect(function() {
        gui.clickButton(buttons.NextButton);
    })
}

Controller.prototype.WelcomePageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.CredentialsPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.IntroductionPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function()
{
    if (installer.fileExists(installer.environmentVariable("PWD") + "/Qt")) {
       installer.setDefaultPageVisible(QInstaller.TargetDirectory, false);
       installer.setDefaultPageVisible(QInstaller.ReadyForInstallation, false);
       installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
       installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);
       installer.setDefaultPageVisible(QInstaller.PerformInstallation, false);
       installer.setDefaultPageVisible(QInstaller.LicenseCheck, false);
       gui.clickButton(buttons.NextButton);
    }
    gui.currentPageWidget().TargetDirectoryLineEdit.setText(installer.environmentVariable("PWD") + "/Qt");
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.ComponentSelectionPageCallback = function() {
    var widget = gui.currentPageWidget();
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.LicenseAgreementPageCallback = function() {
    gui.currentPageWidget().AcceptLicenseRadioButton.setChecked(true);
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.StartMenuDirectoryPageCallback = function() {
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.ReadyForInstallationPageCallback = function()
{
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.FinishedPageCallback = function() {
    var checkBoxForm = gui.currentPageWidget().LaunchQtCreatorCheckBoxForm
    if (checkBoxForm && checkBoxForm.launchQtCreatorCheckBox) {
        checkBoxForm.launchQtCreatorCheckBox.checked = false;
    }
    gui.clickButton(buttons.FinishButton);
}