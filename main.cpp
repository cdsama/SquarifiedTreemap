// Copyright (c) 2019 chendi
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>

#include "stf.hpp"
#include "string_utils.hpp"

int main()
{
    using namespace nana;

    Array<Number> Datas = {6, 6, 4, 3, 2, 2, 1};
    Rectangle Container = {600, 400};
    auto Result = SolveSquarifiedTreemap(Datas, Container);

    paint::font ft("Consolas", 14, true);
    ft.set_default();

    form fm(API::make_center((int)Container.Width, (int)Container.Height), {});
    fm.caption("Squarified Treemap");
    API::track_window_size(fm, {100, 100}, false);
    drawing dw(fm);
    dw.draw([&](paint::graphics &graph) {
        int index = 0;
        for (auto &&i : Result)
        {
            rectangle rt = rectangle((int)round(i.X), (int)round(i.Y), (unsigned int)round(i.Width), (unsigned int)round(i.Height));
            graph.rectangle(rt, true, colors::dark_border);
            graph.rectangle(rectangle(rt.x + 1, rt.y + 1, rt.width - 1, rt.height - 1), false, colors::orange);
            std::stringstream ss;
            ss << Datas[index];
            std::string StrLabel = ss.str();
            auto LabelSize = graph.text_extent_size(std::string_view(StrLabel));
            graph.string(point((int)round(i.X + i.Width / 2 - LabelSize.width / 2), (int)round(i.Y + i.Height / 2 - LabelSize.height / 2)), std::string_view(StrLabel), colors::orange);
            ++index;
        }
    });
    fm.events().resizing([&](const arg_resizing &arg) {
        Container = {(Number)(arg.width), (Number)(arg.height)};
        Result = SolveSquarifiedTreemap(Datas, Container);
        return;
    });

    button btn(fm, rectangle{5, 5, 20, 20});
    btn.caption("+");
    btn.events().click([&](const arg_click &) {
        std::stringstream ss;
        for (auto &&i : Datas)
        {
            ss << i << " ";
        }

        inputbox::text name("", ss.str());
        inputbox inbox(fm, "Please input data.                          ", "New Data");

        if (inbox.show_modal(name))
        {
            auto datastrings = split_string(name.value(), " ");
            Array<Number> NewDatas;
            for (auto &&i : datastrings)
            {
                trim(i);
                if (i.empty())
                {
                    continue;
                }
                Number data = 0;
                try
                {
                    data = std::stof(i);
                }
                catch (const std::exception &)
                {
                    continue;
                }

                if (data > 0)
                {
                    NewDatas.push_back(data);
                }
            }
            if (!NewDatas.empty())
            {
                Datas = NewDatas;
                std::sort(Datas.begin(), Datas.end(), std::greater<>());
                Result = SolveSquarifiedTreemap(Datas, Container);
                dw.update();
            }
        }
    });

    dw.update();
    fm.show();
    exec();
}