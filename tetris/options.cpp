#include <QSettings>
#include <QCoreApplication>
#include "about.h"
#include "options.h"

COptions::COptions()
{
    QCoreApplication::setOrganizationDomain(APP_URL);
    QCoreApplication::setOrganizationName(APP_COMPANY);
    QCoreApplication::setApplicationName(APP_NAME);
}
void COptions::Load()
{
    QSettings settings;
    LayoutLeft = settings.value("LayoutLeft", 200).toInt();
    LayoutTop = settings.value("LayoutTop", 300).toInt();
}
void COptions::Save()
{
    QSettings settings;
    settings.setValue("LayoutLeft", LayoutLeft);
    settings.setValue("LayoutTop", LayoutTop);
}
