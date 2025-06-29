# <img src="C++AcceptanceTesting.svg" style="vertical-align: middle"> C++ Acceptance Testing

## Motivation

Consider the use case of using a C++ program to test a hardware system. For example, we might want to measure a speaker frequency response and test that it is between defined limits.

```c++
    // Define limits:
    const auto test_frequencies_hz = {30, 50, 80, 120, 150};
    const auto upper_dbspl = {100, 100, 100, 100, 100};
    const auto lower_dbspl = {80.f, 90.f, 90.f, 90.f, 90.f};
    // Measure the frequency response:
    auto freq_response_dbspl = test_harness.measure_response(test_frequencies_hz);

    // Now test that it is between the limits:
    for(size_t u = 0U; u != test_frequencies_hz.size(); ++u)
    {
        EXPECT_LE(freq_response_dbspl.at(u), upper_dbspl.at(u));
        EXPECT_GE(freq_response_dbspl.at(u), lower_dbspl.at(u));
    }

```

When this runs we get a test report:
```
    Test passed
```

However, there are a few problems:
1. From an accountability perspective, we don't really know what the test did (just by looking at the output). It could have just printed "Test passed" without measuring anything
2. We don't know how far we are from the limits. It would be nice to see a graph of the measured response and the limits
3. If the "Test passed" is all that is saved, we can't go back later and see what values were actually measured. It could be argued that the actual value measured is irrelevant given it was within the limits. However, saving the value provides extra information. If a manufacturing process was leading to worse 
4. We probably need a test report which links the requirements to the test cases etc. There needs to be a mechanism to link project requirements, test requirements and test cases.


We might want to write something like:
```c++
    add_description(R"(Test that the measured frequency response is between defined limits)");

    // Define limits:
    const auto test_frequencies_hz = {30, 50, 80, 120, 150};
    const auto upper_dbspl = {100, 100, 100, 100, 100};
    const auto lower_dbspl = {80.f, 90.f, 90.f, 90.f, 90.f};
    // Measure the frequency response:
    auto freq_response_dbspl = test_harness.measure_response(test_frequencies_hz);

    test_between(lower_dbspl, freq_response_dbspl, upper_dbspl);
    auto response_plot = figure("Frequency response with limits");
    // ...
    add_graph(response_plot);
    // Add a table of the measurements:
    add_detail(table::from_cols({"Test Frequency [Hz]", "Measured [dBSPL]", "Lower Limit [dBSPL]", "Upper Limit [dBSPL]"}, {test_frequencies_hz, freq_response_dbspl, lower_dbspl, upper_dbspl}));
```

Now we might get a report like the following:

### Test Report

#### Test Traceability
TODO: Table for traceability and link to test cases

#### Frequency Response Measurement

Test that the measured frequency response is between defined limits

TODO: Print asserts

|Test Frequency [Hz] | Measured [dBSPL] |Lower Limit [dBSPL]| Upper Limit [dBSPL]|
|---|---|---|---|
| 30 | 94.7| 80 | 100 |
| 50 | 96.8| 90 | 100 |
| 80 |  98|  90 | 100 |
| 120 | 97.5| 90 | 100 |
| 150 | 98.1 | 90 | 100 |

TODO: Print graph

Note that the test code needs some control over what goes into the report: If the frequency response has a large number of points we still want to test each point individually but might only include the limit plot in the report and not the table of values.

## ReqIF
The [Requirements Interchange Format (ReqIF)](https://www.omg.org/reqif/) is probably the most suitable format to use for requirements traceability since:
- It is an open format
- It has wide industry support
- It supports the capabilities required (linking requirements to test requirements and test cases). 

The `ReqIF` folder contains the requirements for the `C++ Acceptance Testing` project both to define the requirments for this project as well as to serve as an example, development aid and to be used in testing the library.