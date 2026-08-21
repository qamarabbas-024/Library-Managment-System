CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -O2
IFLAGS   := -Iinclude
OUTDIR   := build

# OS detection for cross-platform compatibility
ifeq ($(OS),Windows_NT)
    TARGET  := $(OUTDIR)/LibrarySystem.exe
    TEST_BIN := $(OUTDIR)/test_runner.exe
    MKDIR   := if not exist $(OUTDIR) mkdir $(OUTDIR)
    RM      := if exist $(OUTDIR) rmdir /s /q $(OUTDIR)
else
    TARGET  := $(OUTDIR)/LibrarySystem
    TEST_BIN := $(OUTDIR)/test_runner
    MKDIR   := mkdir -p $(OUTDIR)
    RM      := rm -rf $(OUTDIR)
endif

SOURCES  := \
    src/utils/DateTime.cpp \
    src/utils/Crypto.cpp \
    src/utils/StringUtils.cpp \
    src/utils/Terminal.cpp \
    src/utils/TableFormatter.cpp \
    src/models/Book.cpp \
    src/models/User.cpp \
    src/models/Loan.cpp \
    src/models/Reservation.cpp \
    src/storage/CsvEngine.cpp \
    src/storage/BookRepository.cpp \
    src/storage/UserRepository.cpp \
    src/storage/LoanRepository.cpp \
    src/storage/ReservationRepository.cpp \
    src/storage/AuditLogger.cpp \
    src/services/BookService.cpp \
    src/services/AuthService.cpp \
    src/services/MemberService.cpp \
    src/services/CirculationService.cpp \
    src/services/ReportService.cpp \
    src/ui/LibraryApp.cpp \
    main.cpp

TEST_SOURCES := \
    src/utils/DateTime.cpp \
    src/utils/Crypto.cpp \
    src/utils/StringUtils.cpp \
    src/utils/Terminal.cpp \
    src/utils/TableFormatter.cpp \
    src/models/Book.cpp \
    src/models/User.cpp \
    src/models/Loan.cpp \
    src/models/Reservation.cpp \
    src/storage/CsvEngine.cpp \
    src/storage/BookRepository.cpp \
    src/storage/UserRepository.cpp \
    src/storage/LoanRepository.cpp \
    src/storage/ReservationRepository.cpp \
    src/storage/AuditLogger.cpp \
    src/services/BookService.cpp \
    src/services/AuthService.cpp \
    src/services/MemberService.cpp \
    src/services/CirculationService.cpp \
    src/services/ReportService.cpp \
    tests/test_runner.cpp

.PHONY: all clean run test

all: $(TARGET)

$(TARGET): $(SOURCES)
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) $(IFLAGS) $(SOURCES) -o $(TARGET)
	@echo "Build successful: $(TARGET)"

test: $(TEST_BIN)
	$(CXX) $(CXXFLAGS) $(IFLAGS) $(TEST_SOURCES) -o $(TEST_BIN)
	@echo "Running tests..."
	./$(TEST_BIN)

run: all
	./$(TARGET)

clean:
	@$(RM)
