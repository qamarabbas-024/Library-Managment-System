@echo off
setlocal

set CXX=g++
set STD=-std=c++20
set FLAGS=-Wall -Wextra -O2
set IFLAGS=-Iinclude
set OUTDIR=build
set OUT=%OUTDIR%\LibrarySystem.exe

if "%1"=="clean" (
    echo [CLEAN] Removing build directory...
    if exist %OUTDIR% rmdir /s /q %OUTDIR%
    echo Done.
    exit /b 0
)

echo [BUILD] Library Management System v2.0
echo ============================================

if not exist %OUTDIR% mkdir %OUTDIR%

echo [CC] Compiling source files...

%CXX% %STD% %FLAGS% %IFLAGS% ^
    src\utils\DateTime.cpp ^
    src\utils\Crypto.cpp ^
    src\utils\StringUtils.cpp ^
    src\utils\Terminal.cpp ^
    src\utils\TableFormatter.cpp ^
    src\utils\Barcode.cpp ^
    src\models\Book.cpp ^
    src\models\User.cpp ^
    src\models\Loan.cpp ^
    src\models\Reservation.cpp ^
    src\storage\CsvEngine.cpp ^
    src\storage\BookRepository.cpp ^
    src\storage\UserRepository.cpp ^
    src\storage\LoanRepository.cpp ^
    src\storage\ReservationRepository.cpp ^
    src\storage\AuditLogger.cpp ^
    src\services\BookService.cpp ^
    src\services\AuthService.cpp ^
    src\services\MemberService.cpp ^
    src\services\CirculationService.cpp ^
    src\services\ReportService.cpp ^
    src\services\RecommendationService.cpp ^
    src\services\ReceiptService.cpp ^
    src\ui\LibraryApp.cpp ^
    main.cpp ^
    -o %OUT%

if %errorlevel% neq 0 (
    echo.
    echo [FAILED] Build failed. Check errors above.
    exit /b 1
)

echo.
echo [OK] Build successful!
echo      Output: %OUT%
echo.
echo Run with:  .\%OUT%
