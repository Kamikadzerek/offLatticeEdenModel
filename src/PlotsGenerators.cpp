#include <iostream>
#include <filesystem>
#include "gnuplot-iostream.h"
extern const std::string dataPath;
extern const std::string plotsPath;
void savePlotsMeanRadiusOfLivingCells()
{
    std::string output;
    std::string dataFile;
    std::string dataFilename;
    for (const auto &data: std::filesystem::directory_iterator(dataPath))
    {
        dataFilename = data.path().stem().string();
        if (dataFilename.substr(0, 3) == "Mea")
        {
            dataFile = data.path().string();
            output = plotsPath + "/" + dataFilename;
            Gnuplot gp;
            gp << "set grid\n"
               << "set terminal pdfcairo size 29.7cm, 21cm\n"
               << "set title 'Liczba żywych komórek od średniego ich promienia - Model Edena "
               << (dataFilename[4] == 'S' ? "Kwadraty " : "Okręgi ")
               << "; Wersja:" << dataFilename[11]
               << "; Ilość komórek klastra: " << std::string_view(dataFilename).substr(15, 7)
               << "; Iteracji:" << std::string_view(dataFilename).substr(dataFilename.size() - 7) << "'\n"
               << "set output '" + output + ".pdf'\n"
               << "set key right bottom\n"
               << "set xlabel 'Średni promień'\n"
               << "set ylabel 'Ilość żywych komórek'\n"
               << "plot '" + dataFile + "' using 1:2 pt 1 ps 1 lc rgb 'black' title 'Dane'\n";
        }
    }
}
void savePlotsRoughness()
{
    std::string output;
    std::string dataFile;
    std::string dataFilename;
    for (const auto &data: std::filesystem::directory_iterator(dataPath))
    {
        dataFilename = data.path().stem().string();
        if (dataFilename.substr(0, 3) == "Rgs")
        {
            dataFile = data.path().string();
            output = plotsPath + "/" + dataFilename;
            Gnuplot gp;
            gp << "set grid\n"
               << "set terminal pdfcairo size 29.7cm, 21cm\n"
               << "set title 'Szorstkość powierzchni - Model Edena "
               << (dataFilename[4] == 'S' ? "Kwadraty " : "Okręgi ")
               << "; Wersja:" << dataFilename[11]
               << "; Ilość komórek klastra: " << std::string_view(dataFilename).substr(15, 7)
               << "; Iteracji:" << std::string_view(dataFilename).substr(dataFilename.size() - 7) << "'\n"
               << "set output '" + output + ".pdf'\n"
               << "set key right bottom\n"
               << "set xlabel 'l'\n"
               << "set ylabel 'w(l)'\n"
               << "set logscale y 2\n"
               << "set logscale x 2\n"
               << "f(x) = c*x**a\n"
               << "fit f(x) '" + dataFile + "' using 1:2 via a, c\n"
               << "set label 1 sprintf('a = %3.4f',a) at 64,0.5\n"
               << "plot '" + dataFile + "' using 1:2 pt 1 ps 1 lc rgb 'black' title 'Dane', f(x) title 'Dofitowana funkcja c*x**a'\n";
        }
    }
}
void savePlotsNumberOfCellsEnclousedByRadius()
{
    std::string output;
    std::string dataFile;
    std::string dataFilename;
    for (const auto &data: std::filesystem::directory_iterator(dataPath))
    {
        dataFilename = data.path().stem().string();
        if (dataFilename.substr(0, 3) == "Num")
        {
            dataFile = data.path().string();
            output = plotsPath + "/" + dataFilename;
            Gnuplot gp;
            gp << "set grid\n"
               << "set terminal pdfcairo size 29.7cm, 21cm\n"
               << "set title 'Ilość komórek zamkniętych w okręgu o promieniu R - Model Edena "
               << (dataFilename[4] == 'S' ? "Kwadraty " : "Okręgi ")
               << "; Wersja:" << dataFilename[11]
               << "; Ilość komórek klastra: " << std::string_view(dataFilename).substr(15, 7)
               << "; Iteracji:" << std::string_view(dataFilename).substr(dataFilename.size() - 7) << "'\n"
               << "set output '" + output + ".pdf'\n"
               << "set key right bottom\n"
               << "set xlabel 'R'\n"
               << "set ylabel 'ilość komórek'\n"
               << "plot '" + dataFile + "' using 1:2 pt 1 ps 1 lc rgb 'black' title 'Dane'\n";
        }
    }
}
void saveAllPlots(){
    savePlotsMeanRadiusOfLivingCells();
    savePlotsRoughness();
    savePlotsNumberOfCellsEnclousedByRadius();
}
