﻿#pragma once

#define NOMINMAX

#include <Windows.h>
#include <vector>
#include <algorithm>

#include "TextureBuffer.h"
#include "GfxTypes.h"

namespace gfx
{
	/**
	 * \brief Установка цвета заданного пикселя
	 * \param image Буфер изображения
	 * \param x Номер пикселя в ряду
	 * \param y Номер ряда пикслей в изображении
	 * \param color 
	 */
	void SetPoint(TextureBuffer * image, int x, int y, ColorBGR color);

	/**
	 * \brief Установка цвета заданного пикселя (с проверкой выхода за пределы)
	 * \param image Буфер изображения
	 * \param x Номер пикселя в ряду
	 * \param y Номер ряда пикслей в изображении
	 * \param color 
	 */
	void SetPointSafe(TextureBuffer * image, int x, int y, ColorBGR color);

	/**
	 * \brief Рисование линии (алгоритм предшествующий алгоритму Брезенхэма)
	 * \param image Буфер изображения
	 * \param pointSrc Начальная точка
	 * \param pointDst Конечная точка
	 * \param color Цвет
	 * \param safePoints Использовать функцию SetPointSafe для точек
	 */
	void SetLinePreBresenham(TextureBuffer * image, Vector2D<int> pointSrc, Vector2D<int> pointDst, ColorBGR color, bool safePoints = false);

	/**
	 * \brief Рисование линии
	 * \param image Буфер изображения
	 * \param pointSrc Начальная точка
	 * \param pointDst Конечная точка
	 * \param color Цвет
	 * \param safePoints Использовать функцию SetPointSafe для точек
	 */
	void SetLine(TextureBuffer * image, Vector2D<int> pointSrc, Vector2D<int> pointDst, ColorBGR color, bool safePoints = false);

	/**
	 * \brief Рисование линии (c градиентным цветом)
	 * \param image Буфер изображения
	 * \param pointSrc Начальная точка
	 * \param pointDst Конечная точка
	 * \param colorSrc Цвет начальной точки
	 * \param colorDst Цвет конечной точки
	 * \param safePoints Использовать функцию SetPointSafe для точек
	 */
	void SetLineInterpolated(TextureBuffer * image, Vector2D<int> pointSrc, Vector2D<int> pointDst, Color4f colorSrc, Color4f colorDst, bool safePoints = false);

	/**
	 * \brief Рисование линии (с проверкой выхода начальной и конечной точки за пределы)
	 * \param image Буфер изображения
	 * \param pointSrc Начальная точка
	 * \param pointDst Конечная точка
	 * \param color Цвет
	 */
	void SetLineSafe(TextureBuffer * image, Vector2D<int> pointSrc, Vector2D<int> pointDst, ColorBGR color);

	/**
	 * \brief Рисование линии (с проверкой выхода начальной и конечной точки за пределы)
	 * \param image Буфер изображения
	 * \param pointSrc Начальная точка
	 * \param pointDst Конечная точка
	 * \param colorSrc Цвет начальной точки
	 * \param colorDst Цвет конечной точки
	 */
	void SetLineSafeInterpolated(TextureBuffer * image, Vector2D<int> pointSrc, Vector2D<int> pointDst, Color4f colorSrc, Color4f colorDst);

	/**
	 * \brief Шаблонная функция для нахождения прямоугольной области описывающей точки
	 * \tparam T Тип
	 * \param points Точки 
	 * \return Прямоугольная область
	 */
	template <typename T = int>
	Box2D<T> FindBoundingBox2D(std::vector<Vector2D<T>> points)
	{
		Box2D<T> result;

		Vector2D<T> minX = *std::min_element(points.begin(), points.end(), [](Vector2D<T> elem1, Vector2D<T> elem2) {return elem1.x < elem2.x; });
		Vector2D<T> minY = *std::min_element(points.begin(), points.end(), [](Vector2D<T> elem1, Vector2D<T> elem2) {return elem1.y < elem2.y; });
		Vector2D<T> maxX = *std::max_element(points.begin(), points.end(), [](Vector2D<T> elem1, Vector2D<T> elem2) {return elem1.x < elem2.x; });
		Vector2D<T> maxY = *std::max_element(points.begin(), points.end(), [](Vector2D<T> elem1, Vector2D<T> elem2) {return elem1.y < elem2.y; });

		result.topLeft = { minX.x, minY.y };
		result.bottomRight = { maxX.x, maxY.y };
		return result;
	}

	/**
	 * \brief Шаблонная функция для нахождения прямоугольной области описывающей треугольник
	 * \details Для нахождения описывающего прямоугольника для триогольника оптимальнее использовать именно этот метод (не FindBoundingBox2D)
	 * \tparam T Тип
	 * \param p0 Точка 1
	 * \param p1 Точка 2
	 * \param p2 Точка 3
	 * \return Прямоугольная область
	 */
	template <typename T = int>
	Box2D<T> FindTriangleBoundingBox2D(Vector2D<T> p0, Vector2D<T> p1, Vector2D<T> p2)
	{
		Box2D<T> result;
		result.topLeft = { std::min<T>({ p0.x, p1.x, p2.x }) , std::min<T>({ p0.y, p1.y, p2.y }) };
		result.bottomRight = { std::max<T>({ p0.x, p1.x, p2.x }) , std::max<T>({ p0.y, p1.y, p2.y }) };
		return result;
	}

	/**
	 * \brief Рисование прямоугольника
	 * \param image Буфер изображения
	 * \param box Параметры прямоугольника (2 точки)
	 * \param color Цвет
	 */
	void SetRectangle(TextureBuffer * image, Box2D<int> box, ColorBGR color);

	/**
	 * \brief Рисование треугольника (полигон)
	 * \param image Буфер изображения
	 * \param p0 Точка 1
	 * \param p1 Точка 2
	 * \param p2 Точка 3
	 * \param color Цвет
	 */
	void SetPolygon(TextureBuffer * image, Vector2D<int> p0, Vector2D<int> p1, Vector2D<int> p2, ColorBGR color);

	/**
	 * \brief Рисование треугольника (полигон)
	 * \param image Буфер изображения
	 * \param p0 Точка 1
	 * \param p1 Точка 2
	 * \param p2 Точка 3
	 * \param col0 Цвет точки 1
	 * \param col1 Цвет точки 2
	 * \param col2 Цвет точки 3
	 */
	void SetPolygonInterpolated(TextureBuffer * image, Vector2D<int> p0, Vector2D<int> p1, Vector2D<int> p2, Color4f col0, Color4f col1, Color4f col2);

	/**
	 * \brief Находится ли точка внутри треугольника
	 * \param p Проверяемая точка
	 * \param a Точка треугольника A
	 * \param b Точка треугольника B
	 * \param c Точка треугольника C
	 * \return Да или нет
	 */
	bool IsInTriangle(Vector2D<int> p, Vector2D<int> a, Vector2D<int> b, Vector2D<int> c);

	/**
	 * \brief Находится ли точка внутри треугольника (векторный метод)
	 * \param p Проверяемая точка
	 * \param a Точка треугольника A
	 * \param b Точка треугольника B
	 * \param c Точка треугольника C
	 * \return Да или нет
	 */
	bool IsInTriangleVector(Vector2D<int> p, Vector2D<int> a, Vector2D<int> b, Vector2D<int> c);

	/**
	 * \brief Представление (показ) изображения на поверхности WinAPI окна
	 * \param image Текстурный буфер
	 * \param hWnd Хендл окна
	 */
	void PresentFrame(TextureBuffer * image, HWND hWnd);
}
