
//
// Copyright (C) 2011 Proteomics Center,
//                    Children's Hospital Boston, Boston, MA 02115
//                    Mathias Wilhelm, Marc Kirchner
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "pwiz/data/msdata/MSDataFile.hpp"
#include <iostream>
#include <numeric>
#include <omp.h>

// export OMP_NUM_THREADS=4

using namespace std;
using namespace pwiz::msdata;

void countTIC(const string& filename)
{
    // manipulate your MSData object here
    MSDataFile msd(filename);

    SpectrumListPtr sl = msd.run.spectrumListPtr;
     
    double TIC = 0;
    long int nr_peaks = 0;

    if (sl.get())
    {
        // NOTE: since sl->spectrum is protected by a
        // boost::lock_guard<boost::mutex> lock(readMutex); there is no real
        // parallelization happening here, e.g. everything is blocked at this
        // function call
        cout << "# of spectra: " << sl->size() << endl;
#pragma omp parallel for
        for(unsigned int i = 0; i< sl->size(); i++)
        {
//#pragma omp critical (thread)
//          std::cout << "Thread " << omp_get_thread_num() << " working on spectrum " << i << std::endl;
          SpectrumPtr sptr = sl->spectrum(i, true);
          BinaryDataArrayPtr intensities = sptr->getIntensityArray();
          std::vector<double> data = intensities->data;
#pragma omp critical (acc)
          {
            TIC += std::accumulate(data.begin(), data.end(), 0.0);
            nr_peaks += data.size();
          }
        }
    }
    cout << "total TIC  " << TIC << endl;
    cout << "nr peaks " << nr_peaks << endl;
}

int main(int argc, char* argv[])
{
    try {
        if (argc != 2)
            throw runtime_error(
                    "Usage: tic_pwiz_parallel filename");
        countTIC(argv[1]);
        return 0;
    } catch (exception& e) {
        cerr << e.what() << endl;
    } catch (...) {
        cerr << "Caught unknown exception.\n";
    }
    return 1;
}
