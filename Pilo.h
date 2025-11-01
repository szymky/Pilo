//
// Created by mroeb on 11/1/25.
//

#ifndef PILOTEST_PILO_H
#define PILOTEST_PILO_H


#include  <algorithm>
#include  <cstring>
#include  <iostream>
#include  <unordered_map>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/gpio.h>

template <typename CharT, std::size_t N>
struct fixed_string
{
    CharT data[N]{};

    constexpr fixed_string(const CharT  (&str)[N])
    {
        std::copy_n(str, N, data);
    }
};

template <fixed_string chip_name, fixed_string label>
class GPIO
{
public:
    GPIO()
    {
        chip_fd = open(chip_name.data, O_RDWR);
        if (chip_fd < 0)
        {
            return;
        }
    }
    ~GPIO()
    {
        close(chip_fd);
        for (const auto& [chip, fd] : chip_fds)
            close(fd);
    };

    template<unsigned int... Lines>
    void add_lines()
    {
        (add_line(Lines), ...);
    }

    template<unsigned int Line>
    void write(bool value)
    {
        if (not chip_fds.contains(Line)) return;

        gpiohandle_data data{};
        data.values[0] = value ? 1 : 0;
        ioctl(chip_fds.at(Line), GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
    }

private:
    int chip_fd{-1};
    std::unordered_map<unsigned int, int> chip_fds ;
    void add_line(const unsigned int line)
    {
        gpiohandle_request req{};
        req.lineoffsets[0] = line;
        req.flags = GPIOHANDLE_REQUEST_OUTPUT;
        req.lines = 1;
        std::strcpy(req.consumer_label, label.data);
        if(ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &req) < 0)
        {
            close(chip_fd);
            return;
        }

        chip_fds[line] = req.fd;
    }
};

#endif //PILOTEST_PILO_H