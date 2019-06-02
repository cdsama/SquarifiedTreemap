// Copyright (c) 2019 chendi
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <iostream>
#include <vector>
#include <numeric>
#include <memory>
#include <assert.h>
#include <utility>
#include <algorithm>

using Number = float;
template <typename T>
using Array = std::vector<T>;
template <typename T>
using UniquePtr = std::unique_ptr<T>;
using OStream = std::ostream;
template <typename K, typename V>
using Pair = std::pair<K, V>;
#define MakePair std::make_pair
#define MakeUnique std::make_unique

Number Zero = 0;
Number One = 1;

enum class Orientation
{
    Vertical,
    Horizontal
};

OStream &operator<<(OStream &os, Orientation o)
{
    return os << (o == Orientation::Vertical ? "Vertical" : "Horizontal");
}

struct Rectangle
{
    Number Width;
    Number Height;

    Number X = 0;
    Number Y = 0;

    Orientation GetOrientation() const
    {
        return Width < Height
                   ? Orientation::Vertical
                   : Orientation::Horizontal;
    }

    Number GetAspectRatio() const
    {
        return Width > Height ? Width / Height : Height / Width;
    }

    Rectangle operator-(const Rectangle &Other) const
    {
        assert(Other.Width <= Width);
        assert(Other.Height <= Height);
        assert((Other.Width == Width) || (Other.Height == Height));
        if (Other.Width == Width)
        {
            return {Width, Height - Other.Height, X, Y + Other.Height};
        }
        else
        {
            return {Width - Other.Width, Height, X + Other.Width, Y};
        }
    }

    Rectangle operator*(Number Ratio) const
    {
        Ratio = std::clamp(Ratio, std::numeric_limits<Number>::min(), One);
        if (Width > Height)
        {
            return {Width * Ratio, Height, X, Y};
        }
        else
        {
            return {Width, Height * Ratio, X, Y};
        }
    }

    Rectangle operator*(Pair<Orientation, Number> &v) const
    {
        auto &orientation = v.first;
        auto &Ratio = v.second;
        Ratio = std::clamp(Ratio, std::numeric_limits<Number>::min(), One);
        if (orientation == Orientation::Horizontal)
        {
            return {Width * Ratio, Height, X, Y};
        }
        else
        {
            return {Width, Height * Ratio, X, Y};
        }
    }
};

OStream &operator<<(OStream &os, const Rectangle &v)
{
    return os << "[w:" << v.Width << " h:" << v.Height << ", x:" << v.X << " y:" << v.Y << "]";
}

struct Layout
{
    Rectangle Container;
    Orientation DataOrientation;
    Array<Number> CurrentDatas;
    Number TotalData;
    UniquePtr<Layout> NextLayout;

    Layout *AddData(Number Data)
    {
        if (CurrentDatas.empty())
        {
            CurrentDatas.push_back(Data);
            Rectangle NewData = Container * (Data / TotalData);
            DataOrientation = NewData.GetOrientation();
            if (Container.GetOrientation() == DataOrientation && TotalData > Data)
            {
                NextLayout = MakeUnique<Layout>();
                NextLayout->Container = Container - NewData;
                NextLayout->TotalData = TotalData - Data;
                return NextLayout.get();
            }
        }
        else
        {
            Number FirstRatio = Zero;
            {
                Number CurrentTotal = accumulate(CurrentDatas.begin(), CurrentDatas.end(), Data);
                Rectangle CurrentBlock = Container * (CurrentTotal / TotalData);
                Rectangle LastData = CurrentBlock * MakePair(DataOrientation, (CurrentDatas.back() / CurrentTotal));
                FirstRatio = LastData.GetAspectRatio();
            }
            Number SecondRatio = Zero;
            UniquePtr<Layout> MayBeNextLayout = MakeUnique<Layout>();
            {
                Number CurrentTotal = accumulate(CurrentDatas.begin(), CurrentDatas.end(), Zero);
                Rectangle CurrentBlock = Container * (CurrentTotal / TotalData);
                Rectangle LastData = CurrentBlock * MakePair(DataOrientation, (CurrentDatas.back() / CurrentTotal));
                SecondRatio = LastData.GetAspectRatio();

                MayBeNextLayout->Container = Container - CurrentBlock;
                MayBeNextLayout->TotalData = TotalData - CurrentTotal;
                Rectangle NewData = MayBeNextLayout->Container * (Data / MayBeNextLayout->TotalData);
                MayBeNextLayout->DataOrientation = NewData.GetOrientation();
            }

            if (FirstRatio < SecondRatio)
            {
                CurrentDatas.push_back(Data);
            }
            else
            {
                MayBeNextLayout->CurrentDatas.push_back(Data);
                NextLayout = move(MayBeNextLayout);
                return NextLayout.get();
            }
        }
        return nullptr;
    }

    Array<Rectangle> GetLayoutRectangles() const
    {
        Array<Rectangle> Result;
        Number LastTotal = accumulate(CurrentDatas.begin(), CurrentDatas.end(), Zero);
        Rectangle LastBlock = Container * (LastTotal / TotalData);
        for (auto &&i : CurrentDatas)
        {
            Rectangle ThisBlock = LastBlock * MakePair(DataOrientation, i / LastTotal);
            LastBlock = LastBlock - ThisBlock;
            LastTotal = LastTotal - i;
            Result.push_back(ThisBlock);
        }
        return Result;
    }
};
OStream &operator<<(OStream &os, const Layout &v)
{
    os << "Container:" << v.Container << " DataOrientation:" << v.DataOrientation << " CurrentDatas: [";
    for (auto &&i : v.CurrentDatas)
    {
        os << i << ",";
    }
    os << "] TotalData: " << v.TotalData << "\n";
    if (v.NextLayout != nullptr)
    {
        os << *v.NextLayout;
    }
    return os;
}

Array<Rectangle> TravelLayout(Layout *CurrentLayout, size_t count)
{
    Array<Rectangle> Result;
    Result.reserve(count);
    while (CurrentLayout != nullptr)
    {
        auto Current = CurrentLayout->GetLayoutRectangles();
        Result.insert(Result.end(), Current.begin(), Current.end());
        CurrentLayout = CurrentLayout->NextLayout.get();
    }
    return Result;
}

Array<Rectangle> SolveSquarifiedTreemap(const Array<Number> &Datas, const Rectangle &Container)
{
    Number TotalData = std::accumulate(Datas.begin(), Datas.end(), Zero);
    UniquePtr<Layout> OriginalLayout = MakeUnique<Layout>();
    OriginalLayout->Container = Container;
    OriginalLayout->TotalData = TotalData;
    Layout *CurrentLayout = OriginalLayout.get();
    for (auto &&i : Datas)
    {
        Layout *Next = CurrentLayout->AddData(i);
        if (Next != nullptr)
        {
            CurrentLayout = Next;
        }
    }

    return TravelLayout(OriginalLayout.get(), Datas.size());
}