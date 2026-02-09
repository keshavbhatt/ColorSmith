#include "../include/AboutDialog.h"
#include "../include/version.h"
#include "ui_AboutDialog.h"
#include <QApplication>
#include <QDesktopServices>
#include <QIcon>
#include <QPixmap>
#include <QStyle>
#include <QUrl>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AboutDialog) {
  ui->setupUi(this);

  // Set window title
  setWindowTitle(QString("About %1").arg(COLORSMITH_APP_NAME));

  // Load and set application icon
  QPixmap iconPixmap(":/icons/colorsmith.svg");
  if (!iconPixmap.isNull()) {
    ui->iconLabel->setPixmap(iconPixmap.scaled(64, 64, Qt::KeepAspectRatio,
                                               Qt::SmoothTransformation));
  } else {
    // Fallback: use application style icon
    QIcon appIcon = style()->standardIcon(QStyle::SP_MessageBoxInformation);
    ui->iconLabel->setPixmap(appIcon.pixmap(64, 64));
  }

  ui->appNameLabel->setText(QString("<h2>%1</h2>").arg(COLORSMITH_APP_NAME));
  ui->versionLabel->setText(
      QString("Version %1").arg(COLORSMITH_VERSION_STRING));

  ui->descriptionLabel->setText(
      "A professional color management suite for designers, developers, and digital artists. "
      "Featuring precision color picking, WCAG-compliant contrast checking, gradient creation, and comprehensive palette management.");

  ui->copyrightLabel->setText(
      QString("© 2026 %1").arg(COLORSMITH_ORGANIZATION_NAME));

  ui->additionalInfoLabel->setText("Designed and developed by Keshav Bhatt");

  connect(ui->closeButton, &QPushButton::clicked, this, &AboutDialog::accept);
}

AboutDialog::~AboutDialog() { delete ui; }

void AboutDialog::on_btnDonate_clicked() {
  QDesktopServices::openUrl(
      QUrl("https://www.paypal.com/ncp/payment/KS3TNDR4XLUE2"));
}

void AboutDialog::on_btnMoreApps_clicked() {
  QDesktopServices::openUrl(
      QUrl("https://ktechpit.com/USS/public/products.php"));
}

void AboutDialog::on_btnSourceCode_clicked() {
  QDesktopServices::openUrl(
      QUrl("https://github.com/keshavbhatt/ColorSmith"));
}

