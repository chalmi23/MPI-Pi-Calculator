#include <iostream>
#include <chrono>
#include <mpi.h>

long double calculatePi(int start, long int end, long double& result);

using namespace std;

int main(int argc, char* argv[])
{
    int start = 1;          //Początek przedziału
    int rank = 0, size = 0; // Zmienne do identyfikacji procesów
    long int end = 1000000000;      //deklaracja ilosci elementow
    long double result = 0, wynikKoncowy = 0; // Zmienna do przechowywania wyniku końcowego


    auto timeStart = chrono::high_resolution_clock::now(); //poczatek mierzenia czasu
    calculatePi(start, end, result);                        //obliczanie liczby pi szeregowo
    auto timeStop = chrono::high_resolution_clock::now();   //koniec mierzenia czasu

    cout.precision(10);         //ustawienie 10 liczb po przecinku
    cout << "Obliczona wartosc liczby Pi szeregowo: " << result * 4 << endl;       //wyswietlenie wyniku liczby Pi
    cout << "Czas obliczenia szeregowego w milisekundach: " << chrono::duration_cast<chrono::milliseconds>(timeStop - timeStart).count() << endl;  //wyswietlenie czasu

    result = 0;

    MPI_Init(&argc, &argv);                     // Inicjalizacja MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);       // Identyfikacja bieżącego procesu
    MPI_Comm_size(MPI_COMM_WORLD, &size);       // Określenie liczby procesów

    int interval = end / size;
    int startProcess = rank * interval + 1;     // Przedział startowy dla procesu
    int endProcess = (rank + 1) * interval;     // Przedział końcowy dla procesu

    if (rank == size - 1)
    {
        endProcess = end;                   // Jeśli jesteśmy w ostatnim procesie, to końcowy przedział jest równy end
    }

    auto startTimeAsync = chrono::high_resolution_clock::now();
    calculatePi(startProcess, endProcess, result);                  // Obliczenie wartości Pi rownolegle
    auto endTimeAsync = chrono::high_resolution_clock::now();

    MPI_Reduce(&result, &wynikKoncowy, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);     //zredukowanie wynikow i obliczenie wyniku koncowego

    if (rank == 0)      //wyswietlenie wyniku oraz czasu obliczen rownoleglych
    {
        cout.precision(10);
        cout << "Obliczona wartosc liczby Pi rownolegle: " << wynikKoncowy * 4 << endl;
        cout << "Czas obliczania rownoleglego w milisekundach: " << chrono::duration_cast<chrono::milliseconds>(endTimeAsync - startTimeAsync).count() << endl;
        cout << "Liczba elementow z ktorych policzona byla liczba Pi: " << end << endl; //Wyświetlenie elementów z których liczymy
    }
    MPI_Finalize();     //konczenie komunikacji miedzy procesami
    return 0;
}

long double calculatePi(int start, long int end, long double& result)
{
    double sum = 0;
    double diff = 0;

    for (int i = start; i < end; i++)
    {
        if (i % 4 == 1)
        {
            sum += (1.0 / i);
        }
        else if (i % 4 == 3)
        {
            diff -= (1.0 / i);
        }
    }
    return result += sum + diff;
}