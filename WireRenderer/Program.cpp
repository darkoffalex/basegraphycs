﻿#include <iostream>
#include <Windows.h>

#include "../Base/Gfx.h"
#include "../Base/TextureBuffer.h"
#include "../Base/VertexBuffer.h"

/**
* \brief Оконная процедура (объявление)
* \param hWnd Хендл окна
* \param message Идентификатор сообщения
* \param wParam Параметр сообщения
* \param lParam Парамтер сообщения
* \return Код выполнения
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

/**
 * \brief Рисование "сеточной" модели
 * \param image Буфер изображения
 * \param model Буфер вершин
 * \param color Цвет
 */
void DrawWireModel(gfx::TextureBuffer* image, gfx::VertexBuffer* model, gfx::ColorBGR color = {0,255,0,0});

// Буфер кадра
gfx::TextureBuffer frameBuffer;

// Буффер вершин
gfx::VertexBuffer vertexBuffer;

/**
* \brief Точка входа
* \param argc Кол-во аргументов
* \param argv Агрументы (строки)
* \return Код выхода
*/
int main(int argc, char* argv[])
{
	try
	{
		// Получение хендла исполняемого модуля
		HINSTANCE hInstance = GetModuleHandle(nullptr);

		// Регистрация класса окна
		WNDCLASSEX classInfo;
		classInfo.cbSize = sizeof(WNDCLASSEX);
		classInfo.style = CS_HREDRAW | CS_VREDRAW;
		classInfo.cbClsExtra = 0;
		classInfo.cbWndExtra = 0;
		classInfo.hInstance = hInstance;
		classInfo.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
		classInfo.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
		classInfo.hCursor = LoadCursor(nullptr, IDC_ARROW);
		classInfo.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
		classInfo.lpszMenuName = nullptr;
		classInfo.lpszClassName = L"WireRendererWindow";
		classInfo.lpfnWndProc = WndProc;

		// Если не удалось зарегистрировать класс
		if (!RegisterClassEx(&classInfo)) {
			throw std::runtime_error("ERROR: Can't register window class.");
		}

		// Создание окна
		HWND mainWindow = CreateWindow(
			classInfo.lpszClassName,
			L"WireRenderer",
			WS_OVERLAPPEDWINDOW,
			0, 0,
			800, 600,
			NULL,
			NULL,
			hInstance,
			NULL);

		// Если не удалось создать окно
		if (!mainWindow) {
			throw std::runtime_error("ERROR: Can't create main application window.");
		}

		// Показать окно
		ShowWindow(mainWindow, SW_SHOWNORMAL);

		// Получить размеры клиентской области окна
		RECT clientRect;
		GetClientRect(mainWindow, &clientRect);
		std::cout << "INFO: Client area sizes : " << clientRect.right << "x" << clientRect.bottom << std::endl;

		// Создать буфер кадра по размерам клиенсткой области
		frameBuffer = gfx::TextureBuffer(clientRect.right, clientRect.bottom);
		std::cout << "INFO: Frame-buffer initialized  (size : " << frameBuffer.GetSize() << " bytes)" << std::endl;

		// Загрузить вершины в буфер вершин из файла
		vertexBuffer.LoadFromFile("models/african_head.obj");
		std::cout << "INFO: Vertex-buffer initialized (size: " << vertexBuffer.GetSize() << " bytes, " << vertexBuffer.GetVertices().size() << " model, " << vertexBuffer.GetFaces().size() << " faces)" << std::endl;

		// Построить изображение модели
		DrawWireModel(&frameBuffer, &vertexBuffer, {0,0,255,0});

		// Оконное сообщение (пустая структура)
		MSG msg = {};

		// Вечный цикл (работает пока не пришло сообщение WM_QUIT)
		while (true)
		{
			// Если получено сообщение
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				DispatchMessage(&msg);

				if (msg.message == WM_QUIT) {
					break;
				}
			}

			// Если хендл окна не пуст
			if (mainWindow)
			{
				// Сообщение "перерисовать", чтобы показать обновленный кадр
				SendMessage(mainWindow, WM_PAINT, NULL, NULL);
			}

		}
	}
	catch (std::exception const &ex)
	{
		std::cout << ex.what() << std::endl;
	}

	return 0;
}

/**
* \brief Оконная процедура (реализация)
* \param hWnd Хендл окна
* \param message Идентификатор сообщения
* \param wParam Параметр сообщения
* \param lParam Парамтер сообщения
* \return Код выполнения
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		gfx::PresentFrame(&frameBuffer, hWnd);
		return DefWindowProc(hWnd, message, wParam, lParam);

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

/**
* \brief Рисование "сеточной" модели
* \param image Буфер изображения
* \param model Буфер вершин
* \param color Цвет
*/
void DrawWireModel(gfx::TextureBuffer* image, gfx::VertexBuffer* model, gfx::ColorBGR color)
{
	// Проход по всем полигонам модели (полигон - массив индексов вершин, обычно 3 индекса)
	for (unsigned int i = 0; i < model->GetFaces().size(); i++)
	{
		// Получение индексов текущего полигона
		std::vector<int> faceIndices = model->GetFaces()[i];

		// Проход по всем точкам полигона (если считать что их 3)
		for (unsigned int j = 0; j < 3; j++)
		{
			// Получаем текущую точку и следующую (следующая может оказаться нулевой, завершающей)
			gfx::Vector3D<float> v0 = model->GetVertices()[faceIndices[j]];
			gfx::Vector3D<float> v1 = model->GetVertices()[faceIndices[(j + 1) % 3]];

			// Преобразование однородных координат [-1,1] вершин в оконные координаты [0,ширина]
			// Используем только 2 ости (X и Y). Ось Z (глубина) не задействуется, в рузультате
			// получаем что-то вроде изометрической проекции (без перспективного искажения).
			// Если заменить оси X и Z - получим вид модели с иной стороны
			int x0 = static_cast<int>((v0.x + 1.0f)*(image->GetWidth() / 2.0f));
			int y0 = static_cast<int>((v0.y*-1 + 1.0f)*(image->GetHeight() / 2.0f)); //Учитываем инверсию оси Y, умнажаем на -1
			int x1 = static_cast<int>((v1.x + 1.0f)*(image->GetWidth() / 2.0f));
			int y1 = static_cast<int>((v1.y*-1 + 1.0f)*(image->GetHeight() / 2.0f)); //Учитываем инверсию оси Y, умнажаем на -1

			// Отрисовка безопасной линии
			gfx::SetLineSafe(image, { x0, y0 }, { x1, y1 }, color);
		}
	}
}